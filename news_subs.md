An improvement on the famous reader writer problem.
“m” news source proccesses and “n” subscriber processes, each news source process delivers data “d” to all subscriber processes by calling publish function. 
Meanwhile, every subscriber process can call read_news() function in order to fetch the copy of the published data “d”. 
During this operation, the processes follow the rules given below:
i. A subscriber can only fetch a copy of a news once. If there are not any published news yet, the subscriber process should be suspended.
ii. A news source cannot publish a new data, until the previously published one is fetched by all subscribers. It should wait inside the function until all previous copies are delivered.
