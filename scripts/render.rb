#!/usr/bin/env ruby
#
# Render all dot files in a given directory
#
# Usage: render path/to/dir/

if ARGV.size != 1
    STDERR.puts "Render all dotfiles in a directory"
    STDERR.puts ""
    STDERR.puts "Usage: #{$0} directory/"
    exit(0)
end

path = ARGV.shift
Dir.foreach(path) do |file|
	if file =~ /dot$/
		full = File.join(path,file)
		out = full.gsub(/dot$/,"png")
		
		if not File.exists?(out) or File.ctime(full) > File.ctime(out)
			STDERR.puts "RENDER: #{full}"
			`dot -Tpng #{full} -o #{out}`
		else
			STDERR.puts "SKIP:   #{full}"
		end
	end
end
