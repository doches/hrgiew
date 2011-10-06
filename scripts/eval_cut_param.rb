# Find the best level at which to cut a hierarchy into a flat clustering
#
# Usage: ruby #{$0} path/to/human.dot gold.yaml

@dotfile = ARGV.shift
@gold = ARGV.shift

def eval(n)
	handle = File.basename(@dotfile)
	tempfile = "cut_#{handle}_#{n}.yaml"
  `ruby scripts/dot2clusters.rb #{@dotfile} #{n} > #{tempfile}`
  count = `cat #{tempfile} | grep ':' | wc -l`.to_i
  score = `clusterval -s -g #{@gold} -c #{tempfile}`.to_f
  `rm #{tempfile}`
  return [score,count]
end

scores = []
n = 0
previous = 0
score = 1
while score[0] != previous
  previous = score[0]
  
  score = eval(n)
  puts "#{n}\t#{score.join("\t")}"
  STDERR.puts "#{n}\t#{score.join("\t")}"
  n += 1
  scores.push score
end
