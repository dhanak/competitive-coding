const test = "3,4,3,1,2"

function simulate(fish::AbstractVector{<: Integer},
                  days::Integer;
                  fertility_cycle::Integer = 7,
                  spawn_age::Integer = 8
                 )::Integer
    ages = [count(==(age), fish) for age in 0:spawn_age]
    for day in 1:days
        spawning = ages[1]
        ages = [ages[2:end]; spawning]
        ages[fertility_cycle] += spawning
    end
    return sum(ages)
end

let v = parse.(Int, split(test, ','))
    @assert simulate(v, 80) == 5934
    @assert simulate(v, 256) == 26984457539
end

v = parse.(Int, split(readline("day06.in"), ','))
println("Q1: ", simulate(v, 80))
println("Q2: ", simulate(v, 256))
