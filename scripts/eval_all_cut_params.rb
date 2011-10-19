# Takes a directory containing a number of *.human.dot files and a gold-standard yaml clustering
# and determines the best cluster cut for each.
#
# Usage: ruby #{$0} path/to/dir gold.yaml match-filter

path = ARGV.shift
gold = ARGV.shift
filter = ARGV.shift
Dir.glob(File.join(path,"*.human.dot")).each do |dotfile|
	if dotfile =~ /#{filter}/
		output = File.join(path,"#{File.basename(dotfile).gsub('.human.dot','.ncut')}")
		cmd = "ruby scripts/eval_cut_param.rb #{dotfile} #{gold} > #{output}"
		STDERR.puts cmd
		puts `#{cmd}`.strip
	end
end
