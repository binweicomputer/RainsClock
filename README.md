# RainsClock
...或在命令行上创建新存储库
echo "# RainsClock" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin git@github.com:binweicomputer/RainsClock.git
git push -u origin main
...或从命令行推送现有存储库
git remote add origin git@github.com:binweicomputer/RainsClock.git
git branch -M main
git push -u origin main