# Takes a GraphViz .dot file and a hierarchy level at which to make the cut, and produces a
# flat clustering suitable for evaluation using cluster f-score.
#
# Usage: ruby #{$0} file.dot N > file.yaml

def children_of(node)
  return node if @nodes[node][:children].nil?
  
  @nodes[node][:children].map { |child| children_of(child) }.flatten.uniq
end

if ARGV.size != 2
  STDERR.puts "USAGE: ruby \#{$0} file.dot N > file.yaml"
  exit(1)
end

dotfile = ARGV.shift
cut_level = ARGV.shift.to_i

# Process .dot into some sort of tree
@nodes = {}
@parents = {}
IO.foreach(dotfile) do |line|
  if line =~ /(\S+) (\-\-|\-\>) (\S+);/
    parent,child = $1,$3
    
    @nodes[parent] ||= {}
    @nodes[child] ||= {}
    
    @nodes[parent][:children] ||= []
    @nodes[parent][:children].push child
    
    @parents[child] = parent
  elsif line =~ /(\S+) \[([^\]]+)\]/
    node,attrs = $1,$2.split(/[, ]+/).map { |x| x.split("=").map { |y| y.strip.gsub("\"","") } }
    @nodes[node] ||= {}
    attrs.each do |pair|
      @nodes[node][pair[0].to_sym] = pair[1]
    end
  end
end

# Find the root node (node with no parent)
root = @nodes.keys.reject { |node| not @parents[node].nil? }.pop

# Make the cut
clusters = [root]
cut_level.times do
  clusters = clusters.map { |node| (@nodes[node].nil? or @nodes[node][:children].nil?) ? node : @nodes[node][:children] }.flatten
end

hash = {}
clusters.each_with_index { |node,cluster_index| hash["#{cluster_index}"] = children_of(node).map { |node| @nodes[node][:label] } }
puts hash.to_yaml