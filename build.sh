#!/bin/sh

allArguments=$@

usage() { echo "Usage: $0 -p android|ios [-c]"; echo "  -p -> platform, ios or android (required)"; echo "  -c -> cpp11 support"; 1>&2; exit 1; }

while getopts "p:c" o; do
    case "${o}" in
        p)
            p=${OPTARG}
            if [ "$p" != "ios" ]; then
               if [ "$p" != "android" ]; then
                 usage
               fi
            fi
            ;;
        c)
            c="cpp11"
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${p}" ]; then
    usage
fi

if [ $p == "ios" ]; then
    echo "Building iOS examples..."
    rm -rf "./ios/build"
    rm -rf "./ios/Include"
    ./update.platform.sh $allArguments
    pushd ios
    ./build.sh $allArguments
    resultcode=$?
    popd
elif [ $p == "android" ]; then
    echo "Building Android examples..."
    rm -rf "./android/obj"
    rm -rf "./android/bin"
    # todo -- fix headers so no longer depend on old native-activity-glue
    #rm -rf "./android/libs"
    #./update.platform.sh $allArguments
    pushd android
    ./build.sh $allArguments
    resultcode=$?
    popd
fi

if [ $resultcode = 0 ] ; then
  echo "BUILD SUCCEEDED"
  exit 
else
  echo "BUILD FAILED"
fi

exit $resultcode
