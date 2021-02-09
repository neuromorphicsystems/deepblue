host='pi@192.168.0.9'
path='atis_recorder'

rsync -avz --exclude build ./ $host:$path/
ssh $host "cd $path && premake4 --raspberry-pi gmake && cd build && make"
