require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new('spec')
task :build do
  load 'ext/extconf.rb'
  output = `make`
  raise output unless $? == 0
  File.rename('keychain_rice.bundle', 'lib/keychain_rice/keychain_rice.bundle')
end

task :spec => :build

task :default => :spec