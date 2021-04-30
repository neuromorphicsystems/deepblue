host='pi@20.0.0.3'
path='atis_recorder'

rsync -avz --exclude build ./ $host:$path/
ssh $host "cd $path && premake4 --raspberry-pi gmake && cd build && make"
