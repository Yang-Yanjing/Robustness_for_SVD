@REM 获取远程更新
git fetch origin 
@REM 合并分支
git merge origin/master


git add .

@REM 评论修改
git commit -m "Git_script update"

git push origin master
