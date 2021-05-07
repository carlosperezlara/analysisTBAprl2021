all:
	rootcint -f Dict_analysis.cpp -c `root-config --cflags` -p qabar.h bar.h waveform.h LinkDef.h
	g++ -o run analysis.cc waveform.cxx qabar.cxx bar.cxx Dict_analysis.cpp `root-config --cflags --glibs` -fpermissive -rpath /Users/cperez/external/root_install/lib
	rm Dict_analysis.cpp
	cp Dict_analysis_rdict.pcm bin
