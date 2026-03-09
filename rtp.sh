# pnpm run end-to-end --numberOfSimulation=200
set -e

upload_file="e2e-report.html"
output_file="e2e-report.txt"
$@ 2>&1 | tee "$output_file"
echo "date:$date"
echo "name: $logic_name"
echo "s3 web: $s3_web"
echo "tag: $tag"
echo "cmd: $@"
echo "url: $lark_webhook_url"

if [ $? -eq 0 ]; then
    # Upload the output file to S3
    s3_key="$logic_name/$tag/$date-$upload_file"

    echo "s3_key: $s3_key"

    echo "aws s3 cp \"$upload_file\" \"s3://$s3_bucket_name/$s3_key\" --region \"ap-southeast-1\""
    aws s3 cp "$upload_file" "s3://$s3_bucket_name/$s3_key" --region "ap-southeast-1"

    s3_url="$s3_web/$s3_key"

    # Prepare the Lark message
    lark_message="{
        \"msg_type\": \"text\",
        \"content\": {
        \"text\": \"TAG: $tag\\nSERVICE: $logic_name\\nCOMMAND: $@\\nOUTPUT_URL: $s3_url\"
        }
    }"

    # Send the message to the Lark bot
    curl -X POST "$lark_webhook_url" \
            -H "Content-Type: application/json" \
            -d "$lark_message"

    echo "Output saved to $output_file, uploaded to S3, and link sent to Lark!"
else
    echo "Command failed to execute. No message sent to Lark."
fi