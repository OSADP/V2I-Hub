<!DOCTYPE HTML>
<html>
	<body>
	<script>
		console.log("Shutting Down!");
	</script>
	<?php
		// Need to add line to sudo with 'sudo visudo' command
		// Cmnd_Alias SHUTDOWN_CMDS = /sbin/halt
		// www-data  ALL=(ALL) NOPASSWD: SHUTDOWN_CMDS

		exec('sudo /sbin/halt', $haltoutput);
	?>
	<script>
		console.log("Shutdown has been called");
	</script>
	</body>
</html>