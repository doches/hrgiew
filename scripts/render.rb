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

def render_file(full)
  if full =~ /dot$/
    out = full.gsub(/dot$/,"png")
	
  	if not File.exists?(out) or File.ctime(full) > File.ctime(out)
  		STDERR.puts "RENDER: #{full}"
  		`dot -Tpng #{full} -o #{out}`
  	else
  		STDERR.puts "SKIP:   #{full}"
  	end
	end
end

path = ARGV.shift
if File.exists?(path)
  Dir.foreach(path) { |file| render_file(File.join(path,file)) }
else
  Dir.glob("#{path}*").each { |file| render_file(file) }
end