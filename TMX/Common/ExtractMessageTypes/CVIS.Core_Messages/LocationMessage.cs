using Newtonsoft.Json;
using System;

namespace CVIS.Core.Messages
{
    //[DataContract]
    [Serializable]
    public class LocationMessage
    {
       // [DataMember]
       [JsonProperty (PropertyName= "Id")]
        public Guid Id { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "Time")]
        public long Time { get; set; } //TODO datetime conversion?how does time come from tmx?
                                       // [DataMember]
        [JsonProperty(PropertyName = "Latitude")]
        public double Latitude { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "Longitude")]
        public double Longitude { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "Speed")]
        public double Speed { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "Heading")]
        public double Heading { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "Altitude")]
        public double Altitude { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "NumSatellites")]
        public int NumSatellites { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "HorizontalDOP")]
        public double HorizontalDOP { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "SignalQualityType")]
        public int SignalQualityType { get; set; }
        // [DataMember]
        [JsonProperty(PropertyName = "FixType")]
        public int FixType { get; set; }//fixQuality?

    }
}
