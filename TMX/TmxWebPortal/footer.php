<?php 
function get_year()
{
	$today = getdate(); 
//	print_r($today);
	return $today['year'];
}

?>
</td></table>

</div>
</div>
<div id="footer">Copyright (c) 2014 - <?php echo(get_year())?> <a target="_blank" href="http://www.battelle.org">Battelle Memorial Institute</a>. All rights reserved.</div>

</body>
</html>

<?php mysqli_close($con); ?>
