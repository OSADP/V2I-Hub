var loadingStartTime = new Date();

function showTime(mess)
{
	var device = "Common ";
	var time = new Date();
//	var timeString = time.getHours() + ":" + time.getMinutes() + ":" + time.getSeconds() + "." + time.getMilliseconds();
//	console.log(device + " " + mess + ":" + timeString);
    var currentTimeString = ('0' + time.getHours()).slice(-2) + ":" + ('0' + time.getMinutes()).slice(-2) + ":" + ('0' + time.getSeconds()).slice(-2) + "." + ("00" + time.getMilliseconds()).slice(-3);

	var elapsed = time - loadingStartTime;
	timeString = elapsed / 1000;
	console.log(currentTimeString + " " + device + " " + mess + " Elapsed:" + timeString);
}
