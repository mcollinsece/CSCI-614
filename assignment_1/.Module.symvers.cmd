cmd_/home/mbc/assignment_1/Module.symvers :=  sed 's/ko$$/o/'  /home/mbc/assignment_1/modules.order | scripts/mod/modpost -m     -o /home/mbc/assignment_1/Module.symvers -e -i Module.symvers -T - 
