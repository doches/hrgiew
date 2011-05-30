# Generate a fake corpus
#
# Usage: ruby gen_corpus.rb <percent_noise : 0.4> <line_count : 2000> <filename : data/noise_4>
#
# Produces: data/noise_4 and data/noise_4.gold

percent_noise = ARGV.shift.to_f
line_count = ARGV.shift.to_i
handle = ARGV.shift

clusters = 10
words_per_cluster = [2,5,10,3,5,4]

words = []
char = 'a'
(0..clusters-1).each { |i| 
    set = []
    
    (0..(words_per_cluster[(i%words_per_cluster.size)]-1)).each { |x|
        set.push "#{char}#{x}"
    }
    
    words.push set
    char = (char[0] + 1).chr
}

features = words.map { |set| (0..10).map { |i| (rand*1000).to_i } }
all = features.flatten

fout = File.open(handle,'w')
line_count.times do 
    index = (rand*words.size).to_i
    set = words[index]
    word = set[(rand*set.size).to_i]
    fset = features[index]
    
    fout.print "#{word} "
    vector = (0..5).map { |i| (rand > percent_noise ? fset[(rand*fset.size).to_i] : all[(rand*all.size).to_i]) }
    
    fout.puts vector.join(" ")
end
fout.close

gold = {}
words.each_with_index do |set,i|
    gold[i] = set
end
fout = File.open("#{handle}.gold",'w')
fout.puts gold.to_yaml
fout.close