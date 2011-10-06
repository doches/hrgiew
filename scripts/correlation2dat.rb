# Looks for all .correlation files in a given directory and produces a GNUPlot-friendly
# .dat file 
#
# Usage: ruby #{$0} path/to/dir > experiment.dat

pairs = []
Dir.glob(File.join(ARGV.shift,"*.correlation")).each do |file|
	score = IO.readlines(file).join("").strip.to_f
	pairs.push [File.basename(file).split(".").shift.gsub("_",".").to_f,score] if score > 0
end
puts pairs.sort { |a,b| a[0] <=> b[0] }.map { |x| x.join("\t") }.join("\n")
