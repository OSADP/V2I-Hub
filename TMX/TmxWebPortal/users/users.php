<?php 
$page_Title = "TMX System Users";
$page_AccessLevel = 3;
?>
<?php include("../header.php"); ?>

<script>

function editUser(username)
{
	var form = document.createElement("form");
	form.setAttribute("method", "post");
	form.setAttribute("action", "<?php echo($c_PathPrefix) ?>/users/editUser.php");

	var field = document.createElement("input");
	field.setAttribute("type", "hidden");
	field.setAttribute("name", "username");
	field.setAttribute("value", username);

	form.appendChild(field);
	document.body.appendChild(form);
	form.submit();
}

</script>


<h2>User List</h2>

<?php

$query = "SELECT * FROM `user`";
$results = sqlQuery($con, $query, "Error loading users");
?>

<table class="data" border="2px">
<tr>
	<th>User Name</th>
	<th>Access Level</th>
	<th>Operations</th>
</tr>

<?php
while($row = mysqli_fetch_array($results)) {
	echo "<tr>";
	echo "<td width=300px>" . $row['username'] . "</td>";
	echo "<td>" . getAccessLevelString($row['accessLevel']) . "</td>";
	if ($row['username'] == $_SESSION['username']) 
	{
		echo "<td><i>Active User</i></td>";
	}
	else
	{ 
		echo "<td><button onClick=\"return editUser('".$row['username']."')\" style=\"min-width:100px;\">Edit</button>";
	}
	echo "</tr>";
}
?>
</table>



<h2>Create New User</h2>

<form action="createUser.php" method="post">
	<input type="hidden" name="execute" value="1"/>
	<label for="username">User Name:</label>
	<input type="text" name="username" id="username"/><br/>
	<label for="accessLevelStr">Access Level:</label>
	<select name="accessLevelStr" id="accessLevelStr">
		<option><?php echo(getAccessLevelString(1)); ?></option>
		<option><?php echo(getAccessLevelString(2)); ?></option>
		<option><?php echo(getAccessLevelString(3)); ?></option>
	</select><br/>
	<input type="submit" value="Create"/>
</form>

<?php include("../footer.php"); ?>
