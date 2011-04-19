# Generate a fake corpus

percent_noise = ARGV.shift.to_f

line_count = ARGV.empty? ? 200 : ARGV.shift.to_i

clusters = 10
words_per_cluster = [2,5,10,3,5,4]

words = []
char = 'a'
(0..clusters-1).each { |i| 
    set = []
    
    (0..(words_per_cluster[(rand*words_per_cluster.size)]-1)).each { |x|
        set.push "#{char}#{x}"
    }
    
    words.push set
    char = (char[0] + 1).chr
}

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