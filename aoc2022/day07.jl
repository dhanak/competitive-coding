test = raw"""
$ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k
"""

function parse_input(input::AbstractString)::Dict
    lines = split(input, '\n'; keepempty = false)

    # parse listings
    pwd = ""
    entries = Dict{String, Any}("/" => String[])
    for line in lines
        if line[1:4] == raw"$ cd"
            pwd = abspath(joinpath(pwd, line[6:end]))
        elseif line[1:4] != raw"$ ls"
            (size, name) = split(line, ' ')
            path = joinpath(pwd, name)
            push!(entries[pwd], path)
            entries[path] = size == "dir" ? String[] : parse(Int, size)
        end
    end

    # compute sizes recursively
    dirs = [k for (k, v) in entries if v isa Vector]
    return map(sort!(dirs; rev = true)) do path
        size = entries[path] = sum(p -> entries[p], entries[path])
        return path => size
    end |> Dict
end

function q1(dir_sizes::Dict{String, Int})::Int
    return sum(size for (_, size) in dir_sizes if size <= 100_000)
end

function q2(dir_sizes::Dict{String, Int})::Int
    free_up = dir_sizes["/"] - 40_000_000
    sizes = sort!(collect(values(dir_sizes)))
    return sizes[findfirst(>=(free_up), sizes)]
end

let dir_sizes = parse_input(test)
    @assert dir_sizes["/a/e"] == 584
    @assert dir_sizes["/a"] == 94853
    @assert dir_sizes["/d"] == 24933642
    @assert dir_sizes["/"] == 48381165

    @assert q1(dir_sizes) == 95437
    @assert q2(dir_sizes) == 24933642
end

dir_sizes = parse_input(read("day07.in", String))
println("Q1: ", q1(dir_sizes))
println("Q2: ", q2(dir_sizes))
