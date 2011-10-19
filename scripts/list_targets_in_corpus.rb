# Read in a target corpus from STDIN and print out a list of target words to STDOUT
#
# Usage: ruby #{$0} < path/to/file.target_corpus > file.nouns

targets = []

STDIN.each_line do |line|
	target = line.split(" ").shift
	targets.push target.to_sym if not target.nil?
	targets.uniq!
end

puts targets.join("\n")
