# Look in a directory for multiple thresholding trials and compute the average score for each value
#
# Usage: ruby #{$0} dir/

dir = ARGV.shift
trials = {}
Dir.glob(File.join(dir,"*.correlation")).each do |file|
	if File.basename(file) =~ /(\d+\.)?(\d\_\d+)/
		trial = $1
		param = $2.gsub('_','.').to_f
		score = `cat #{file}`.to_f
		trials[param] ||= []
		trials[param].push score
	end
end

best = [0,0]

trials.sort { |a,b| a[0] <=> b[0] }.each do |k,v|
	v.each { |x| best = [k,x] if x > best[1] }
	mean = v.inject(0) { |s,x| s += x }/v.size.to_f
	
	mean = 0 if mean < 0
	
	# StdDev
	stddev = (v.inject(0) { |sum,x| sum += (x-mean)**2 } / v.size.to_f)**0.5
	
	puts "#{k}\t#{mean}\t#{stddev}"
end

STDERR.puts "BEST: #{best.join("\t")}"
