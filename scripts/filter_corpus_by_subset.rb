# Takes a subset.yaml or list of target words as an argument, reads a target corpus from
# STDIN, and outputs that target corpus to STDOUT with only the specified target words 
# remaining.
#
# Usage: ruby #{$0} subset.yaml < path/to/corpus.target_corpus > path/to/output.target_corpus

subset = ARGV.shift
if subset =~ /yaml$/
	subset = YAML.load_file(subset).values.flatten.uniq.map { |x| x.downcase.to_sym }
else
	subset = IO.readlines(subset).map { |x| x.strip.downcase.to_sym }
end

STDIN.each_line do |line|
	target = line.split(/\W+/).shift.to_sym
	puts line.strip if subset.include?(target)
end
