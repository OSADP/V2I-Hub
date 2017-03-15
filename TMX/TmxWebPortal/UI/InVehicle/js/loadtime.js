var loadingStartTime = new Date();

function showTime(mess)
{
	var device = "SMC ";
	var time = new Date();
//	var timeString = time.getHours() + ":" + time.getMinutes() + ":" + time.getSeconds() + "." + time.getMilliseconds();
//	console.log(device + " " + mess + ":" + timeString);

	var elapsed = time - loadingStartTime;
	timeString = elapsed / 1000;;
	console.log(device + " " + mess + " Elapsed:" + timeString);
}
