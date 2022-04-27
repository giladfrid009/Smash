SUBMITTERS := 314615113_322784729
COMPILER := g++
COMPILER_FLAGS := --std=c++11 -Wall
HDRS := Commands.h Smash.h Parser.h SigHandlers.h Identidiers.h JobList.h JobEntry.h Special.h
SRCS := Commands.cpp Smash.cpp Parser.cpp SigHandlers.cpp JobList.cpp main.cpp JobEntry.cpp Special.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
SMASH_BIN := smash

$(SMASH_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^

zip: $(SRCS) $(HDRS)
	zip $(SUBMITTERS).zip $^ submitters.txt Makefile

clean:
	rm -rf $(SMASH_BIN) $(OBJS) $(TESTS_OUTPUTS) 
	rm -rf $(SUBMITTERS).zip
