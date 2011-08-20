#!/usr/bin/env ruby

Description = "Takes a path to a results directory and intellintly applies wordmaps within"
executable = ($0 =~ /rb$/ ? "ruby #{$0}" : $0)
Usage = "#{executable} path/to/dir/"

@debug = false

if ARGV.size != 1
  STDERR.puts Description
  STDERR.puts ""
  STDERR.puts Usage
  exit(1)
end

dir = ARGV.shift
Dir.glob(File.join(dir,"*.wordmap")) do |file|
	if file =~ /^(.+)\.wordmap$/
		handle = $1
		cmd = "./apply_wordmap #{file} #{handle}"
		`#{cmd}`
	end
end
