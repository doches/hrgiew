# Run a bunch of trials with randomly generated corpora

require 'fileutils'
include FileUtils

noise_levels = %w{0.0 0.2 0.4 0.6 0.8 1.0}.map { |x| x.to_f }
sizes = %w{1000 2000 4000 8000}.map { |x| x.to_i }
cPerLine = [1,5,10]
cPostSamples = [1000,2000,4000,8000,16000]
allEquilSamples = 2000
equilCount = 25
rootpath = "noise_trials"
saveInterval = 500

sizes.each do |size|
  noise_levels.each do |noise|
    STDERR.puts "\n\t#{(noise*100).to_i}% noise, #{size} lines\n"
    
    # Generate noise corpus
    handle = "data/noise_#{(noise*10).to_i}_#{size}"
    if not File.exists?(handle)
      STDERR.puts "Generating #{handle}..."
      `ruby scripts/gen_corpus.rb #{noise} #{size} #{handle}`
    end
    
    cPerLine.each do |perLine|
      STDERR.puts "\n\t#{perLine} resamples per document"
      
      # Create directory for trial
      dirpath = File.join(rootpath,"noise_#{(noise*10).to_i}_#{size}_#{perLine}")
      mkdir_p(dirpath)
    
      # Produce a dendrogram (and graph) from noise corpus
      STDERR.puts "Building dendrogram..."
      final = "#{handle.split("/").pop}.#{size}"
      `./hierize #{handle} #{dirpath} #{saveInterval} #{perLine}` if not File.exists?(File.join(dirpath,"#{final}.dendrogram"))
      
      cPostSamples.each do |postSamples|
        STDERR.puts "\n\t#{postSamples} post-training resamples"
        
        # Resample final dendrogram
        final_graph = File.join(dirpath,"#{final}.graph")
        STDERR.puts "Resampling..."
        cmd = "./resample #{final_graph} #{File.join(dirpath,"#{final}.dendrogram")} #{dirpath} best_#{postSamples} #{saveInterval} #{postSamples}"
        `#{cmd}` if not File.exists?(File.join(dirpath,"best_#{postSamples}.dendrogram"))
        
        # Build consensus hierarchy
        STDERR.puts "Building consensus hierarchy..."
        prefix = File.join(dirpath,"consensus_#{(noise*10).to_i}_#{size}_#{perLine}_#{postSamples}")
        cmd = "./consensus #{final_graph} #{File.join(dirpath,"best_#{postSamples}.dendrogram")} #{allEquilSamples} #{equilCount} #{prefix}"
        `#{cmd}` if not File.exists?("#{prefix}.consensus.dot")
        
        # Apply wordmap
        STDERR.puts "Applying wordmap..."
        cmd = "./apply_wordmap #{File.join(dirpath,"#{final}.wordmap")} #{File.join(dirpath,"consensus_")}"
        `#{cmd}` if not File.exists?("#{prefix}.consensus.human.dot")
      end  
        
      # Render graphs
      STDERR.puts "Rendering..."
      cmd = "./render #{dirpath}"
      `#{cmd}`
    end
  end
end