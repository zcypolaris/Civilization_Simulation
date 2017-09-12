# Civilization_Simulation
Try to simulate the evolution process of civilizations in the universe.

# make and check result
gcc main.c -Wall -o simulation \
./simulation | tee log \
cat log | grep "*1号\|round" \
# principles 
1.星系位置固定 \
2.每个回合科技自然增长 \
3.每个文明有friend和enermy \
4.有一定科技爆炸几率 \
5.每回和最大动作次数限制 \
6.每回合优先打击enermy \
7.科技指数上限 \
8.探测范围和科技指数成正比 \
9.双方友好度相加小于100则发生战争 \
10.灭掉文明科技指数会有较大提升 \
11.战争胜利科技指数上升，失败下降 \
12.科技水平超过某一水平，友好度会逐渐下降  \
13.达到科学极限后，友好度大幅度下降 \
14.神级文明为超越科学极限的文明，出现几率极小 \
