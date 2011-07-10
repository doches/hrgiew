#!/usr/bin/env ruby

Description = "Takes an inferred hierarchy and a gold-standard hierarchy (both in human-friendly dot format) and compute the cluster f-score"
executable = ($0 =~ /rb$/ ? "ruby #{$0}" : $0)
Usage = "#{executable} candidate.human.dot gold.human.dot"

@debug = false

if ARGV.size != 2
  STDERR.puts Description
  STDERR.puts ""
  STDERR.puts Usage
  exit(1)
end

def clusters_from_dot(path_to_dot)
  clusters = {}
  nodes = {}
  IO.foreach(path_to_dot) do |line|
    if line =~ /(\S+)\s?\-[\>\-]\s?(\S+)/ # Parse edges (foo -> bar)
      parent = $1
      child = $2
      clusters[parent] ||= []
      clusters[parent].push child.gsub(";","")
    elsif line =~ /(\S+)\s?\[.*label=\"([^\"]+\")/ # Labeled nodes
      label = $1
      value = $2
      shape = "none"
      if line =~ /\S+\s?\[.*shape\s?=\s?[^a-zA-Z0-9]?([a-zA-Z0-9]+)/
        shape = $1
      end
      
      if label =~ /LEAF/ or shape == "none" # This is a leaf node...
        nodes[label] = value.gsub("\"","").gsub(";","")
      end
    end
  end
  
  changed = true
  while changed
    changed = false
    clusters.each_pair do |name,children|
      size = children.size
      clusters[name] = children.map { |child| clusters.keys.include?(child) ? clusters[child] : child }.flatten
      if size != clusters[name].size
        changed = true
      end
    end
  end
  
  clusters.each_pair do |name,children|
    clusters[name] = children.map { |id| nodes[id].to_sym }
  end
  
  return clusters
end

def closest_cluster(cluster,all_clusters)
  all_clusters.map { |other| [(cluster&other).size / (cluster|other).size.to_f, other] }.sort { |a,b| a[0] <=> b[0] }.pop[1]
end

def fmeasure(cluster,all_gold_clusters)
  scored = all_gold_clusters.map { |other|
    intersect = cluster & other
    precision = (intersect.size / cluster.size.to_f)
    recall = (intersect.size / other.size.to_f)
    [(2*precision*recall).to_f/(precision+recall), other]
  }
  
  scored.reject! { |pair| pair[0].nan? }
  
  scored.sort { |a,b| a[0] <=> b[0] }.pop
end

candidate_path = ARGV.shift
gold_path = ARGV.shift

candidate = clusters_from_dot(candidate_path)
gold = clusters_from_dot(gold_path)

interim = candidate.values.inject(0) do |sum,cluster|
  f = fmeasure(cluster,gold.values)
  if @debug
    puts "\n\n #### \n"
    print "cluster (#{cluster.size}): "
    p cluster
    print "\noverlap (#{(f[1] & cluster).size}): "
    p f[1] & cluster
    print "\nexcess (#{(f[1] - cluster).size}):  "
    p (f[1] - cluster)
    print "\nnomatch (#{(cluster - f[1]).size}): "
    p (cluster - f[1])
    print "\n  SCORE: #{f[0]}"
  end
  sum += f[0]
end
interim /= candidate.values.size.to_f

puts interim
