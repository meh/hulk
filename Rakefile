require 'rake'
require 'rake/clean'

CC      = 'clang'
CFLAGS  = "-Wall -Werror-implicit-function-declaration -Wno-format -std=gnu11 -Iinclude #{ENV['CFLAGS']}"
LDFLAGS = "#{ENV['LDFLAGS']}"

SOURCES = FileList['source/**/*.c']
OBJECTS = SOURCES.ext('o')

CLEAN.include(OBJECTS)
CLOBBER.include('hulk')

task :default => 'hulk'

file 'hulk' => OBJECTS do
	sh "#{CC} -o hulk #{OBJECTS} #{LDFLAGS}"
end

task :image, :format do |t, args|
	unless args[:format]
		raise ArgumentError, "you have to pass a format"
	end

	sh 'mkdir -p test'
	sh 'dd if=/dev/zero of=test.img bs=1048576 count=10'
	sh "mkfs.#{args[:format]} -F test.img"
	sh 'sudo mount -o loop test.img test'
	sh 'dd if=/dev/urandom of=test/a bs=1048576 count=1'
	sh 'dd if=/dev/urandom of=test/b bs=1048576 count=3'
	sh 'mkdir -p test/c/d/e'
	sh 'dd if=/dev/urandom of=test/c/d/e/f bs=1048576 count=4'
	sh 'sudo umount test'
	sh 'rmdir test'
end

rule '.o' => '.c' do |t|
	sh "#{CC} #{CFLAGS} -o #{t.name} -c #{t.source}"
end
