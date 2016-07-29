using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace CVIS.Core.Messages
{
    [Serializable]
    public class ApplicationDataMessage
    {
        [JsonProperty(PropertyName = "Id")]
        public Guid Id { get; set; }
        [JsonProperty(PropertyName = "Timestamp")]
        public long Timestamp { get; set; }  //ms since epoch
        [JsonProperty(PropertyName = "AppId")]
        public int AppId { get; set; }
        [JsonProperty(PropertyName = "EventID")]
        public Guid? EventID { get; set; }
        [JsonProperty(PropertyName = "InteractionId")]
        public Guid? InteractionId { get; set; }
        [JsonProperty(PropertyName = "IntersectionId")]
        public int IntersectionId { get; set; }
        [JsonProperty(PropertyName = "DataCode")]
        public int DataCodeId { get; set; }
        [JsonProperty(PropertyName = "Data")]
        public string Data { get; set; }
    }
}
