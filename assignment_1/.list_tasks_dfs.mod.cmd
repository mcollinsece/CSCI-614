cmd_/home/mbc/assignment_1/list_tasks_dfs.mod := printf '%s\n'   list_tasks_dfs.o | awk '!x[$$0]++ { print("/home/mbc/assignment_1/"$$0) }' > /home/mbc/assignment_1/list_tasks_dfs.mod
