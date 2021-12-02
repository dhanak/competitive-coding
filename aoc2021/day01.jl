depths = parse.(Int, readlines("day01.in"))
println("Q1: ", count(>(0), diff(depths)))
avgs = depths[1:end-2] .+ depths[2:end-1] .+ depths[3:end]
println("Q2: ", count(>(0), diff(avgs)))
