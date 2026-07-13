local M = {}
local Unpack = table.unpack or unpack

local mask = 0xffffffff
local bit = {}
function bit.band(...)
    local result = mask
    for index = 1, select("#", ...) do result = result & select(index, ...) end
    return result & mask
end
function bit.bxor(...)
    local result = 0
    for index = 1, select("#", ...) do result = result ~ select(index, ...) end
    return result & mask
end
function bit.bor(...)
    local result = 0
    for index = 1, select("#", ...) do result = result | select(index, ...) end
    return result & mask
end
function bit.bnot(value) return (~value) & mask end
function bit.rshift(value, count) return (value & mask) >> count end
function bit.lshift(value, count) return ((value & mask) << count) & mask end
function bit.rrotate(value, count)
    count = count % 32
    return bit.bor(bit.rshift(value, count), bit.lshift(value, 32 - count))
end

local constants = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
}

local function Add(...)
    local result = 0
    for index = 1, select("#", ...) do result = bit.band(result + select(index, ...), 0xffffffff) end
    return result
end
local function Byte(value) return value % 256 end

function M.Hash(value)
    assert(type(value) == "string", "SHA-256 input must be a string")
    local high = math.floor(#value / 0x20000000)
    local low = bit.band(#value * 8, 0xffffffff)
    local padding = (56 - ((#value + 1) % 64)) % 64
    value = value .. string.char(0x80) .. string.rep("\0", padding) ..
        string.char(Byte(bit.rshift(high, 24)), Byte(bit.rshift(high, 16)), Byte(bit.rshift(high, 8)), Byte(high),
            Byte(bit.rshift(low, 24)), Byte(bit.rshift(low, 16)), Byte(bit.rshift(low, 8)), Byte(low))

    local h = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 }
    local w = {}
    for offset = 1, #value, 64 do
        for index = 0, 15 do
            local a, b, c, d = value:byte(offset + index * 4, offset + index * 4 + 3)
            w[index] = bit.bor(bit.lshift(a, 24), bit.lshift(b, 16), bit.lshift(c, 8), d)
        end
        for index = 16, 63 do
            local x, y = w[index - 15], w[index - 2]
            local s0 = bit.bxor(bit.rrotate(x, 7), bit.rrotate(x, 18), bit.rshift(x, 3))
            local s1 = bit.bxor(bit.rrotate(y, 17), bit.rrotate(y, 19), bit.rshift(y, 10))
            w[index] = Add(w[index - 16], s0, w[index - 7], s1)
        end
        local a, b, c, d, e, f, g, hh = Unpack(h)
        for index = 0, 63 do
            local s1 = bit.bxor(bit.rrotate(e, 6), bit.rrotate(e, 11), bit.rrotate(e, 25))
            local choice = bit.bxor(bit.band(e, f), bit.band(bit.bnot(e), g))
            local t1 = Add(hh, s1, choice, constants[index + 1], w[index])
            local s0 = bit.bxor(bit.rrotate(a, 2), bit.rrotate(a, 13), bit.rrotate(a, 22))
            local majority = bit.bxor(bit.band(a, b), bit.band(a, c), bit.band(b, c))
            local t2 = Add(s0, majority)
            hh, g, f, e, d, c, b, a = g, f, e, Add(d, t1), c, b, a, Add(t1, t2)
        end
        h = { Add(h[1], a), Add(h[2], b), Add(h[3], c), Add(h[4], d), Add(h[5], e), Add(h[6], f), Add(h[7], g), Add(h[8], hh) }
    end
    local parts = {}
    for _, word in ipairs(h) do parts[#parts + 1] = string.format("%08x", word) end
    return table.concat(parts)
end

return M
