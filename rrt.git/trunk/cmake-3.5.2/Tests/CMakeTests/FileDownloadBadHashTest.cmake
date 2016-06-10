set(url "file:///home/slee198/RRT/rrt.git/trunk/cmake-3.5.2/Tests/CMakeTests/FileDownloadInput.png")
set(dir "/home/slee198/RRT/rrt.git/trunk/cmake-3.5.2/Tests/CMakeTests/downloads")

file(DOWNLOAD
  ${url}
  ${dir}/file3.png
  TIMEOUT 2
  STATUS status
  EXPECTED_HASH SHA1=5555555555555555555555555555555555555555
  )
