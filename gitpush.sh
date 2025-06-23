read -p "Please enter a commit message : " msg

git add .
git commit -m "$msg"
git push origin main
