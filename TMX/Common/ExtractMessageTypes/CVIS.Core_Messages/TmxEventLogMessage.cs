using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace CVIS.Core.Messages
{
    //[DataContract]
    [Serializable]
    public class TmxEventLogMessage
    {
     
        [JsonProperty(PropertyName = "Id")]
        public Guid Id { get; set; }
   
        [JsonProperty(PropertyName = "Timestamp")]
        public string Time { get; set; }
        [JsonProperty(PropertyName = "Description")]
        public string Description { get; set; }
        [JsonProperty(PropertyName = "Source")]
        public string Source { get; set; }
        [JsonProperty(PropertyName = "LogLevel")]
        public string LogLevel { get; set; }
    }
    }