git checkout master
git pull 
git fetch
git merge develop

git tag --force -a $1 -m"Merge release $1"
git push origin master
git push origin --force --tags
git checkout develop

