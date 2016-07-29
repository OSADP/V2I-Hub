using Newtonsoft.Json;
using System;

namespace CVIS.Core.Messages
{

    [Serializable]
    public class ApplicationMessage
    {
        [JsonProperty(PropertyName = "Id")]
        public Guid Id { get; set; }
        [JsonProperty(PropertyName = "Timestamp")]
        public long Time { get; set; }  //ms since epoch
        [JsonProperty(PropertyName = "AppId")]
        public int AppId { get; set; }
        [JsonProperty(PropertyName = "EventID")]
        public Guid? EventID { get; set; }
        [JsonProperty(PropertyName = "DisplayDuration")]
        public int? DisplayDuration { get; set; }//in milliseconds
        [JsonProperty(PropertyName = "Severity")]
        public int? Severity { get; set; } //maps to AppMessageTypes
        [JsonProperty(PropertyName = "EventCode")]
        public int? EventCode { get; set; }
        [JsonProperty(PropertyName = "InteractionId")]
        public Guid? InteractionId { get; set; }
        [JsonProperty(PropertyName = "CustomText")]
        public string CustomText { get; set; }


    }
}
