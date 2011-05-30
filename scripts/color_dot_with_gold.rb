# Read in GraphViz output and a gold yaml file, and output modified GraphViz output
# with nodes colored according to the gold standard
#
# Usage: ruby color_dot_with_gold.rb path/to/file.dot path/to/corpus.gold

dot = IO.readlines(ARGV.shift)
gold = YAML.load_file(ARGV.shift)

colors = %w{aliceblue antiquewhite aquamarine azure beige bisque blanchedalmond blue blueviolet brown burlywood cadetblue chartreuse chocolate coral cornflowerblue cornsilk crimson cyan darkgoldenrod darkgreen darkkhaki darkolivegreen darkorange darkorchid darksalmon darkseagreen darkslateblue darkslategray darkturquoise darkviolet deeppink deepskyblue dimgray dimgrey dodgerblue firebrick floralwhite forestgreen gainsboro ghostwhite gold goldenrod gray}.sort { |a,b| a.size <=> b.size }

leaves = {}
dot.each do |line|
  if line =~ /LEAF(\d+) \[label=\"([^\"]+)\"\];/
    leaves[$1] = $2
  end
end

colormap = {}
gold.each_pair do |category,words|
  color_index = 0
  color = colors[color_index]
  colors.delete color
  words.each { |word| colormap[word] = color }
end

while dot.pop.strip != "}"
end

puts dot.join("")
leaves.each_pair { |key,label| puts "\tLEAF#{key} [color=\"#{colormap[label]}\", style=filled];" }
puts "}"