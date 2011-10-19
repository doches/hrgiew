# Takes a directory and an output prefix, looks for all *.ncut files, and 
# produces a dotfile and gnuplot file using the output prefix illustrating
# the data
#
# Usage: ruby #{$0} path/to/files output/prefix

path = ARGV.shift
prefix = ARGV.shift

scores = {}
counts = {}

trials = []
old_param = nil
Dir.glob(File.join(path,"*.ncut")) do |file|
  if file =~ /(\d+_\d+)/
    param = $1
    old_param ||= param
    if param != old_param
      STDERR.puts "WARNING: #{file} does not match previous parameter setting \"#{old_param}\""
    end
  else
    STDERR.puts "WARNING: can't determine parameter setting from #{file}"
  end
  
  tree = []
  IO.foreach(file) do |line|
    cut,score,count = *(line.strip.split(/\s+/))
    cut,score,count = cut.to_i,score.to_f,count.to_i
    
    scores[cut] ||= []
    scores[cut].push score
    
    counts[cut] ||= []
    counts[cut].push count
    
    tree.push [cut,score,count]
  end
  trials.push tree
end

datfile = "#{prefix}.dat"

# Scores
fout = File.open(datfile,'w')
scores.sort { |a,b| a[0] <=> b[0] }.each do |pair|
  cut,list = *pair
  mean = list.inject(0) { |s,x| s += x}/list.size.to_f
  stddev = (list.inject(0) { |s,x| s += (x-mean)**2}/list.size.to_f) ** 0.5
  fout.puts "#{cut}\t#{mean}\t#{stddev}"
end
fout.close

# Counts
# counts.sort { |a,b| a[0] <=> b[0] }.each do |pair|
#   cut,list = *pair
#   mean = list.inject(0) { |s,x| s += x}/list.size.to_f
#   stddev = (list.inject(0) { |s,x| s += (x-mean)**2}/list.size.to_f) ** 0.5
#   puts "#{cut}\t#{mean}\t#{stddev}"
# end