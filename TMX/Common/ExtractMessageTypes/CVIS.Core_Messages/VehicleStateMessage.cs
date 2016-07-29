using System;
using Newtonsoft.Json;

namespace CVIS.Core.Messages
{
    //[DataContract]
    [Serializable]
    public class VehicleStateMessage
    {
        // [DataMember]
        [JsonProperty(PropertyName = "Id")]
        public Guid Id { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "Timestamp")]
        public long Time { get; set; } //TODO datetime conversion?how does time come from tmx?
        // [DataMember]
        [JsonProperty(PropertyName = "VehicleParamId")]
        public int VehicleParamId { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "PreState")]
        public int PreState { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "PostState")]
        public int PostState { get; set; }
    }
}
