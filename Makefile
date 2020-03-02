.PHONY: clean All

All:
	@echo "----------Building project:[ Snake - Debug ]----------"
	@cd "Snake" && "$(MAKE)" -f  "Snake.mk" && "$(MAKE)" -f  "Snake.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ Snake - Debug ]----------"
	@cd "Snake" && "$(MAKE)" -f  "Snake.mk" clean
