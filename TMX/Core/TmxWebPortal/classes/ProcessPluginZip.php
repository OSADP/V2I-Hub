<?php

class ProcessPluginZip
{
	public $message;
	public $jsonObj;
	public $directory;
	public $alreadyExists;

	public function processZipFile($target_path, $source, $target_file, $sqlConnection, $overwritePlugin = false)
	{
		$this->jsonObj = NULL;
		$this->alreadyExists = false;

		if(move_uploaded_file($source, $target_file)) {
			$zip = new ZipArchive();
			$x = $zip->open($target_file);
			if ($x === true) {

				$numFiles = $zip->numFiles;

				$contains_manifest = false;
				$manifestFileName = "manifest.json";

				for( $i = 0; $i < $numFiles; $i++ )
				{ 
	    			$stat = $zip->statIndex( $i ); 
	    			$entryName = basename( $stat['name'] );

	    			if( $entryName == $manifestFileName)
	    			{
	    				$contains_manifest = true;

	    				$contents = $zip->getFromIndex($i);

	    				$this->jsonObj = json_decode($contents);
	    			}
				}

				if($contains_manifest)
				{
					$name = $this->jsonObj->name;
					
					$rows = 0;

					if($overwritePlugin == FALSE)
					{

						$query = "SELECT * FROM `plugin` WHERE `name` = '".$name."'";

						$results = mysqli_query($sqlConnection, $query);


						$rows = mysqli_num_rows($results);
					}
					else
					{

						$query = "UPDATE `plugin` SET `version`='" . $this->jsonObj->version."' WHERE `name`='".$this->jsonObj->name."'";

						$results = mysqli_query($sqlConnection, $query);
	

					}

					if ($rows <=0) {
						$target_directory = $target_path . str_replace(" ", "", $name);
						$this->directory = $target_directory;
						$zip->extractTo($target_directory);

						chmod($this->directory . $this->jsonObj->exeLocation, 0755);

						$this->message = "Your .zip file was uploaded and unpacked.";
					}
					else{
						$this->message = "Plugin already exists.";
						$this->alreadyExists = true;
					}
				}
				else
				{
					$this->message = "Zip file missing manifest.json";
				}
				$zip->close();
		
				unlink($target_file);
			}
			
		} else {	
			$this->message = "There was a problem with the upload. Please try again.";
		}
	}
}

?>