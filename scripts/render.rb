#!/usr/bin/env ruby
#
# Render all dot files in a given directory
#
# Usage: render path/to/dir/

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
