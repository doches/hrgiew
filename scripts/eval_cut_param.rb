# Find the best level at which to cut a hierarchy into a flat clustering
#
# Usage: ruby #{$0} path/to/human.dot gold.yaml

@dotfile = ARGV.shift
@gold = ARGV.shift

def eval(n)
  `ruby scripts/dot2clusters.rb #{@dotfile} #{n} > cut_#{n}.yaml`
  score = `clusterval -s -g #{@gold} -c cut_#{n}.yaml`.to_f
  `rm cut_#{n}.yaml`
  return score
end

scores = []
n = 0
previous = 0
score = 1
while score != previous
  previous = score
  
  score = eval(n)
  puts "#{n}\t#{score}"
  n += 1
  scores.push score
end