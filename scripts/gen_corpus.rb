# Generate a fake corpus

percent_noise = ARGV.shift.to_f

line_count = ARGV.empty? ? 200 : ARGV.shift.to_i

words = [
    %w{a1 a2 a3},
    %w{b1 b2},
    %w{c1 c2 c3},
    %w{z1 z2 z3 z4}
]

features = words.map { |set| (0..10).map { |i| (rand*1000).to_i } }
all = features.flatten

line_count.times do 
    index = (rand*words.size).to_i
    set = words[index]
    word = set[(rand*set.size).to_i]
    fset = features[index]
    
    print "#{word} "
    vector = (0..5).map { |i| (rand > percent_noise ? fset[(rand*fset.size).to_i] : all[(rand*all.size).to_i]) }
    
    puts vector.join(" ")
end