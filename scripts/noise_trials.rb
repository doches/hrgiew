# Run a bunch of trials with randomly generated corpora
noise_levels = %w{0.0 0.2 0.4 0.6 0.8 1.0}.map { |x| x.to_f }
sizes = %w{1000 2000 4000 8000}.map { |x| x.to_i }
cPerLine = [1,2,5]
cPostSamples = [0,100,200,400,800,1600]
allEquilSamples = 400
equilCount = 8

sizes.each do |size|
  noise_levels.each do |noise|
    handle = "data/noise_#{(noise*10).to_i}_#{size}"
    `ruby scripts/gen_corpus.rb #{noise} #{size} #{handle}`
    
    cPerLine.each do |perLine|
      cPostSamples.each do |postSamples|
        trial = "#{handle.split("/").pop}-#{perLine}.#{postSamples}"
        puts trial
        `./learner #{handle} #{perLine} #{postSamples} #{allEquilSamples} #{equilCount}`
        `mv consensus.dot #{trial}.orig.dot`
        `ruby scripts/color_dot_with_gold.rb #{trial}.orig.dot #{handle}.gold > #{trial}.dot`
        `dot #{trial}.dot -Tpng -o #{trial}.png`
      end
    end
  end
end