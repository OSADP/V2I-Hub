using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CVIS.Core.Messages
{
    //[DataContract]
    [Serializable]
    public class EpcwDiagnosticMessage
    {
        ///current ingress region of the intersection.
        public int Ingress { get; set; }
        ///Calculated expected egress region of the intersection.
        public int Egress { get; set; }
        ///StageType Enum to identify where we are in terms of traveling through the intersection.
        public int Stage { get; set; }
        ///Excerpts of spat message IF it resulted in suppressing an alert.
        public string SpatExcerpt { get; set; }
    }
}
